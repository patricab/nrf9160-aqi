package aqi;

import java.net.InetSocketAddress;
import java.util.*;

import javax.jmdns.JmDNS;
import javax.jmdns.ServiceInfo;

import org.apache.commons.cli.*;

import org.eclipse.jetty.server.Server;
import org.eclipse.jetty.servlet.ServletHolder;
import org.eclipse.jetty.webapp.WebAppContext;

import org.eclipse.californium.core.network.config.NetworkConfig;
import org.eclipse.californium.core.network.config.NetworkConfig.Keys;
import org.eclipse.californium.elements.util.SslContextUtil;
import org.eclipse.californium.scandium.config.DtlsConnectorConfig;

import org.eclipse.leshan.core.LwM2m;
import org.eclipse.leshan.core.model.ObjectLoader;
import org.eclipse.leshan.core.model.ObjectModel;
import org.eclipse.leshan.core.node.codec.DefaultLwM2mNodeDecoder;
import org.eclipse.leshan.core.node.codec.DefaultLwM2mNodeEncoder;
import org.eclipse.leshan.core.node.codec.LwM2mNodeDecoder;
import org.eclipse.leshan.core.util.SecurityUtil;
import org.eclipse.leshan.core.observation.*;
import org.eclipse.leshan.core.request.*;
import org.eclipse.leshan.core.response.*;
import org.eclipse.leshan.core.node.*;
import org.eclipse.leshan.server.registration.*;
import org.eclipse.leshan.server.observation.ObservationListener;
import org.eclipse.leshan.server.queue.PresenceListener;
import org.eclipse.leshan.server.californium.LeshanServer;
import org.eclipse.leshan.server.californium.LeshanServerBuilder;
import org.eclipse.leshan.server.model.LwM2mModelProvider;
import org.eclipse.leshan.server.model.VersionedModelProvider;
import org.eclipse.leshan.server.redis.RedisRegistrationStore;
import org.eclipse.leshan.server.redis.RedisSecurityStore;
//import org.slf4j.Logger;
//import org.slf4j.LoggerFactory;

import redis.clients.jedis.Jedis;
import redis.clients.jedis.JedisPool;
import redis.clients.jedis.util.Pool;

import aqi.servlet.*;
import aqi.ReadTimerTask;
import aqi.Database;


public class AQIServer 
{
	public static Timer timer;
	public static Database airqdb = new Database("airq", "airqpass");

	private final static String USAGE = "java -jar aqi-server.jar [OPTION]\n\n";

	public static void main( String[] args )
	{
		Options options = new Options();

		options.addOption("h", "help", false, "Display help information.");
		
		options.addOption("lh", "coaphost", true, "Set the local CoAP address.\n  Default: any local address.");
		options.addOption("lp", "coapport", true,
		        String.format("Set the local CoAP port.\n  Default: %d.", LwM2m.DEFAULT_COAP_PORT));
		options.addOption("slh", "coapshost", true, "Set the secure local CoAP address.\nDefault: any local address.");
		options.addOption("slp", "coapsport", true,
		        String.format("Set the secure local CoAP port.\nDefault: %d.", LwM2m.DEFAULT_COAP_SECURE_PORT));

		options.addOption("wh", "webhost", true, "Set the HTTP address for web server.\nDefault: any local address.");
		options.addOption("wp", "webport", true, "Set the HTTP port for web server.\nDefault: 8080.");
	/*
		options.addOption("mdns", "publishDNSSdServices", false,
		        "Publish leshan's services to DNS Service discovery" + RPKChapter);
		options.addOption("pubk", true,
		        "The path to your server public key file.\n The public Key should be in SubjectPublicKeyInfo format (DER encoding).");
		options.addOption("prik", true,
		        "The path to your server private key file.\nThe private key should be in PKCS#8 format (DER encoding)."
		         + X509Chapter);
		options.addOption("cert", true, "The path to your server certificate file.\n"
		        + "The certificate Common Name (CN) should generally be equal to the server hostname.\n"
		        + "The certificate should be in X509v3 format (DER encoding).");
	*/

		HelpFormatter formatter = new HelpFormatter();
		formatter.setWidth(120);
		formatter.setOptionComparator(null);

		LeshanServerBuilder builder = new LeshanServerBuilder();
		final LeshanServer leshanServer = builder.build();

		DtlsConnectorConfig.Builder dtlsConfig = new DtlsConnectorConfig.Builder();
		dtlsConfig.setMaxRetransmissions(42);
		dtlsConfig.setRetransmissionTimeout(1000);


		leshanServer.getRegistrationService().addListener(new RegistrationListener() {
			public void registered(Registration registration, Registration previousReg,
				                    Collection<Observation> previousObsersations) {
				System.out.println("new device: " + registration);
				timer = new Timer();
				//timer.schedule(new ReadTimerTask(leshanServer, registration), 1, 3000);
				timer.schedule(new ReadTimerTask(leshanServer, registration), 2500);
				//try {
				//	ObserveResponse response = leshanServer.send(registration, new ObserveRequest(3300,0,5700));
				//} catch (Exception e) {}
				
			}

			public void updated(RegistrationUpdate update,
			                    Registration updatedReg, Registration previousReg) {
				System.out.println("device is still here: " + updatedReg.getEndpoint());
				//timer.schedule(new ReadTimerTask(leshanServer, updatedReg), 1000);
			}

			public void unregistered(Registration registration, Collection<Observation> observations,
				                      boolean expired, Registration newReg) {
				System.out.println("device left: " + registration.getEndpoint() + observations);
				timer.cancel();
			}
		});

		leshanServer.getObservationService().addListener(new ObservationListener () {
			public void onResponse(Observation observation, Registration registration, 
				ObserveResponse response) {
				System.out.println("Receive notification from " + observation.getPath() 
					              + " value " + response.getContent().toString());
		
				//airqdb.observation(rresponse, registration.getEndpoint());
				airqdb.observation(response, registration.getEndpoint(), leshanServer, registration);
			}

			public void cancelled(Observation observation) {
			}


			public void onError(Observation observation, Registration registration, Exception error) {
			}

			public void newObservation(Observation observation, Registration registration) {
			}
		});

		leshanServer.getPresenceService().addListener(new PresenceListener () {
			public void onSleeping(Registration registration) {
				System.out.println("onSleeping " + registration);
			}

			public void onAwake(Registration registration) {
				System.out.println("onAwake " + registration);
			}
		});

		 // Parse arguments
		CommandLine cl;
		try {
			cl = new DefaultParser().parse(options, args);
		} catch (ParseException e) {
			System.err.println("Parsing failed.  Reason: " + e.getMessage());
			formatter.printHelp(USAGE, options);
			return;
		}

		// Print help
		if (cl.hasOption("help")) {
			formatter.printHelp(USAGE, options);
			return;
		}

		// Abort if unexpected options
		if (cl.getArgs().length > 0) {
			System.err.println("Unexpected option or arguments : " + cl.getArgList());
			formatter.printHelp(USAGE, options);
			return;
		}

		// Abort if all RPK config is not complete
		boolean rpkConfig = false;
		if (cl.hasOption("pubk")) {
			if (!cl.hasOption("prik")) {
				System.err.println("pubk, prik should be used together to connect using RPK");
				formatter.printHelp(USAGE, options);
				return;
			} else {
				rpkConfig = true;
			}
		}

		// Abort if all X509 config is not complete
		boolean x509Config = false;
		if (cl.hasOption("cert")) {
			if (!cl.hasOption("prik")) {
				System.err.println("cert, prik should be used together to connect using X509");
				formatter.printHelp(USAGE, options);
				return;
			} else {
				x509Config = true;
			}
		}

		// Abort if prik is used without complete RPK or X509 config
		if (cl.hasOption("prik")) {
			if (!rpkConfig && !x509Config) {
				System.err.println("prik should be used with cert for X509 config OR pubk for RPK config");
				formatter.printHelp(USAGE, options);
				return;
			}
		}

		// Get mDNS publish switch
		Boolean publishDNSSdServices = cl.hasOption("mdns");

		// get local address
		String localAddress = cl.getOptionValue("lh");
		String localPortOption = cl.getOptionValue("lp");
		Integer localPort = null;
		if (localPortOption != null) {
			localPort = Integer.parseInt(localPortOption);
		}

		// get secure local address
		String secureLocalAddress = cl.getOptionValue("slh");
		String secureLocalPortOption = cl.getOptionValue("slp");
		Integer secureLocalPort = null;
		if (secureLocalPortOption != null) {
			secureLocalPort = Integer.parseInt(secureLocalPortOption);
		}

		// get http address
		String webAddress = cl.getOptionValue("wh");
		String webPortOption = cl.getOptionValue("wp");
		int webPort = 8080;
		if (webPortOption != null) {
			webPort = Integer.parseInt(webPortOption);
		}

		InetSocketAddress jettyAddr;
		if (webAddress == null) {
			jettyAddr = new InetSocketAddress(webPort);
		} else {
			jettyAddr = new InetSocketAddress(webAddress, webPort);
		}
		Server webServer = new Server(jettyAddr);
		WebAppContext root = new WebAppContext();
		root.setContextPath("/");
		root.setResourceBase(AQIServer.class.getClassLoader().getResource("webapp").toExternalForm());
		root.setParentLoaderPriority(true);
		webServer.setHandler(root);

		// Create Servlet
		EventServlet eventServlet = new EventServlet(leshanServer, 5684);
		ServletHolder eventServletHolder = new ServletHolder(eventServlet);
		root.addServlet(eventServletHolder, "event/*");

		ServletHolder clientServletHolder = new ServletHolder(new ClientServlet(leshanServer));
		root.addServlet(clientServletHolder, "/api/clients/*");

	/*ServletHolder securityServletHolder;
		if (publicKey != null) {
			securityServletHolder = new ServletHolder(new SecurityServlet(securityStore, publicKey));
		} else {
			securityServletHolder = new ServletHolder(new SecurityServlet(securityStore, serverCertificate));
		}
		root.addServlet(securityServletHolder, "/api/security/*");
	*/

		ServletHolder objectSpecServletHolder = new ServletHolder(
		new ObjectSpecServlet(leshanServer.getModelProvider(), leshanServer.getRegistrationService()));
		root.addServlet(objectSpecServletHolder, "/api/objectspecs/*");

		/*// Register a service to DNS-SD
		if (publishDNSSdServices) {

			// Create a JmDNS instance
			JmDNS jmdns = JmDNS.create(InetAddress.getLocalHost());

			// Publish Leshan HTTP Service
			ServiceInfo httpServiceInfo = ServiceInfo.create("_http._tcp.local.", "leshan", webPort, "");
			jmdns.registerService(httpServiceInfo);

			// Publish Leshan CoAP Service
			ServiceInfo coapServiceInfo = ServiceInfo.create("_coap._udp.local.", "leshan", localPort, "");
			jmdns.registerService(coapServiceInfo);

			// Publish Leshan Secure CoAP Service
			ServiceInfo coapSecureServiceInfo = ServiceInfo.create("_coaps._udp.local.", "leshan", secureLocalPort, "");
			jmdns.registerService(coapSecureServiceInfo);
		}*/

		// Start Jetty & Leshan
		try {
			leshanServer.start();
			webServer.start();
			System.out.println("Web server started at: " + webServer.getURI());
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
