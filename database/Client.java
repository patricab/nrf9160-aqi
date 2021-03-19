import java.util.*;
import java.lang.*;

public class Client {
	private String endpoint;
	private String registrationId;
	private String registrationDate;
	private String lastUpdate;
	private String address;
	private int lifetime;
	private String rootPath;
	private List<Object> objectLinks;
	private Boolean secure;

	public String getEndpoint() {
		return endpoint;
	}

	public String getRegistrationId() {
		return registrationId;
	}

	public String getRegistrationDate() {
		return registrationDate;
	}

	public String getAddress() {
		return address;
	}

	public List<Object> getObjectLinks() {
		return objectLinks;
	}

	public void setEndpoint(String endpoint) {
		this.endpoint = endpoint;
	}

	public void setRegistrationId(String registrationId) {
		this.registrationId = registrationId;
	}

	public void setRegistrationDate(String registrationDate) {
		this.registrationDate = registrationDate;
	}

	public void setAddress(String address) {
		this.address = address;
	}

	public void setLifetime(int lifetime) {
		this.lifetime = lifetime;
	}

	public void setRootPath(String rootPath) {
		this.rootPath = rootPath;
	}

	public void setSecure(Boolean secure) {
		this.secure = secure;
	}
}