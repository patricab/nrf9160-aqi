package aqi;

import java.util.*;
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



public class ReadTimerTask extends TimerTask {
	LeshanServer server;
	Registration registration;

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
			//ReadResponse response = server.send(registration, new ReadRequest(id1,id2,id3));
			ObserveResponse response = server.send(registration, new ObserveRequest(id1, id2, id3));
			if (response.isSuccess()) {
				System.out.println("Value:" + response.getContent().toString());
			}else {
				System.out.println("Failed to read:" + response.getCode() + " " + response.getErrorMessage());
			}
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void run() {
		//sendReq(3,0,13);
		sendReq(3303, 0, 5700);
		//sendReq(3303, 0, 5518);

		sendReq(3304, 0, 5700);
		//sendReq(3304, 0, 5518);

		System.out.println("Run done");
	}
}