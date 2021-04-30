package aqi;

import java.util.*;
import java.util.concurrent.TimeUnit;
import java.time.*;
import java.nio.ByteBuffer;

import org.eclipse.leshan.core.util.Hex;
import org.eclipse.leshan.core.util.StringUtils;
import org.eclipse.leshan.server.californium.LeshanServer;
import org.eclipse.leshan.core.observation.*;
import org.eclipse.leshan.core.request.*;
import org.eclipse.leshan.core.response.*;
import org.eclipse.leshan.core.node.*;
import org.eclipse.leshan.server.registration.*;
import org.eclipse.leshan.core.model.ResourceModel.Type;
import org.eclipse.leshan.core.node.codec.CodecException;

import aqi.Database;

public class ReadTimerTask extends TimerTask {
	LeshanServer server;
	Registration registration;

	public static Database airqdb = new Database("airq", "airqpass");

	public ReadTimerTask(LeshanServer srv, Registration reg) {
		server = srv;
		registration = reg;
	}

	protected Object getValue(ReadResponse response) {
		Type type = ((LwM2mSingleResource)response.getContent()).getType();
		Object value = ((LwM2mSingleResource)response.getContent()).getValue();
		System.out.println("" + type + value.getClass());
		switch (type) {
			case OPAQUE:
				byte[] bytes = (byte []) value;
				if (bytes.length == 4)
					return ByteBuffer.wrap(bytes).getFloat();
				else if (bytes.length == 8)
					return ByteBuffer.wrap(bytes).getDouble();
				else
					return null;
			default:
				break;

		}
		return value;
	}

	public void sendReq(int id1, int id2, int id3) {
		try {
			ReadResponse response = server.send(registration, new ReadRequest(id1, id2, id3));
			//ObserveResponse response = server.send(registration, new ObserveRequest(id1, id2, id3));
			if (response.isSuccess()) {
				System.out.println("Value:" + response.getContent().toString());
				airqdb.observation(response, registration.getEndpoint(), server,
				                   registration, id1, id2, id3);
			}else {
				System.out.println("Failed to read:" + response.getCode() + " " + response.getErrorMessage());
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
			airqdb.logError(registration.getEndpoint(), registration.getAddress().toString(), registration.getId(), e.toString());
		}
	}

	// public void setDate() {
	// 	try {
	// 		ReadResponse response = server.send(registration, new WriteRequest(3, 0, 13, new Date.now()));
	// 		if (response.isSuccess()) {
	// 			System.out.println("Value:" + response.getContent().toString());
	// 			airqdb.observation(response, registration.getEndpoint(), server,
	// 			                   registration, 3, 0, 13);
	// 		}else {
	// 			System.out.println("Failed to read:" + response.getCode() + " " + response.getErrorMessage());
	// 		}
	// 	} catch (InterruptedException e) {
	// 		e.printStackTrace();
	// 		airqdb.logError(registration.getEndpoint(), registration.getAddress().toString(), registration.getId(), e.toString());
	// 	}
	// }

	@Override
	public void run() {
		try
		{
			//sendReq(3,0,13);
			//Thread.sleep(1000);
			sendReq(3300, 0, 5700); // Gas sensor
			Thread.sleep(500);
			sendReq(3300, 1, 5700); // Concentration sensor
			Thread.sleep(500);
			sendReq(3300, 2, 5700); // Particulate sensor 2.5PM
			Thread.sleep(500);
			sendReq(3300, 3, 5700); // Particulate sensor 10PM
			Thread.sleep(500);
			sendReq(3300, 4, 5700); // Particulate sensor Typisk PM
			Thread.sleep(500);
			sendReq(3303, 0, 5700); // Temperature sensor
			Thread.sleep(500);
			sendReq(3304, 0, 5700); // Humidity sensor
			//Thread.sleep(1000);
			//sendReq(3335, 0, 5700); // Colour sensor
		}
		catch(InterruptedException ex)
		{
			Thread.currentThread().interrupt();
		}

		System.out.println("Read all sensor data");
	}
}