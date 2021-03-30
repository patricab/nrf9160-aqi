
public class Observe { //static
	private String ep;
	private String res;
	private Val val;

	public int getId() {
		return val.getId();
	}

	public String getValue() {
		return val.getValue();
	}

	public Val getVal() {
		return val;
	}
	
	public String getEp() {
		return ep;
	}

	public String getRes() {
		return res;
	}
}
