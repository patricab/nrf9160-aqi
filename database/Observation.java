
public class Observation { //static
	private Content content;
	private boolean failure;
	private String  status;
	private boolean success;
	private boolean valid;

	public int getId() {
		return content.getId();
	}

	public String getValue() {
		return content.getValue();
	}

	public Content getContent() {
		return content;
	}
	
	public boolean getFailure() {
		return failure;
	}

	public String getStatus() {
		return status;
	}

	public boolean getSuccess() {
		return success;
	}

	public boolean getValid() {
		return valid;
	}
}
