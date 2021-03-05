package aqi;

import org.eclipse.leshan.server.californium.LeshanServer;
import org.eclipse.leshan.server.californium.LeshanServerBuilder;
/**
 * Leshan Server
 *
 */
public class AQIServer
{
	public static void main( String[] args )
	{
		LeshanServerBuilder builder = new LeshanServerBuilder();
		LeshanServer server = builder.build();
		server.start();
	}
}
