package aqi;

import java.util.*;

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
//import org.slf4j.Logger;
//import org.slf4j.LoggerFactory;
import org.eclipse.leshan.server.redis.RedisRegistrationStore;
import org.eclipse.leshan.server.redis.RedisSecurityStore;
import redis.clients.jedis.Jedis;
import redis.clients.jedis.JedisPool;
import redis.clients.jedis.util.Pool;

import aqi.ReadTimerTask;
import aqi.Database;


public class AQIServer 
{
	public static Timer timer;
	public static Database airqdb = new Database("airq", "airqpass");

	public static void main( String[] args )
	{
		LeshanServerBuilder builder = new LeshanServerBuilder();
		final LeshanServer server = builder.build();

		server.getRegistrationService().addListener(new RegistrationListener() {
			public void registered(Registration registration, Registration previousReg,
				                    Collection<Observation> previousObsersations) {
				System.out.println("new device: " + registration);
				timer = new Timer();
				//timer.schedule(new ReadTimerTask(server, registration), 1, 3000);
				timer.schedule(new ReadTimerTask(server, registration), 2500);
				//try {
				//	ObserveResponse response = server.send(registration, new ObserveRequest(3300,0,5700));
				//} catch (Exception e) {}
				
			}

			public void updated(RegistrationUpdate update, Registration updatedReg, Registration previousReg) {
				System.out.println("device is still here: " + updatedReg.getEndpoint());
				//timer.schedule(new ReadTimerTask(server, updatedReg), 1000);
			}

			public void unregistered(Registration registration, Collection<Observation> observations, boolean expired,
				                     Registration newReg) {
				System.out.println("device left: " + registration.getEndpoint() + observations);
				timer.cancel();
			}
		});

		server.getObservationService().addListener(new ObservationListener () {
			public void onResponse(Observation observation, Registration registration, 
				ObserveResponse response) {
				System.out.println("Receive notification from " + observation.getPath() 
					              + " value " + response.getContent().toString());
		
				try {
					ReadResponse rresponse = server.send(registration, new ReadRequest(3303,0,5518));
					Object value = ((LwM2mSingleResource)rresponse.getContent()).getValue();
					//java.util.Date date = (java.util.Date) value;
					System.out.println("date: " + value);
				} catch (Exception e) {}

				//airqdb.observation(rresponse, registration.getEndpoint());
				airqdb.observation(response, registration.getEndpoint());
			}

			public void cancelled(Observation observation) {
		}


		public void onError(Observation observation, Registration registration, Exception error) {
		}

			public void newObservation(Observation observation, Registration registration) {
			}
		});

		server.getPresenceService().addListener(new PresenceListener () {
			public void onSleeping(Registration registration) {
				System.out.println("onSleeping " + registration);
			}

			public void onAwake(Registration registration) {
				System.out.println("onAwake " + registration);
			}
		});

		server.start();
	}
}
