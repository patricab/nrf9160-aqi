//STEP 1. Import required packages
import java.sql.*;
import java.time.*;

public class JavaDBCom {
	private static String user;
	private static String pass;

	public JavaDBCom(String user, String pass) {
		this.user = user;
		this.pass = pass;
	}


	// JDBC driver name and database URL
	static final String JDBC_DRIVER = "org.postgresql.Driver";  
	static final String DB_URL = "jdbc:postgresql://localhost:5432/airqdb";


	public static void insertTS(ZonedDateTime time, String ep, int obj, int inst, int res, double val)
	{
		Statement stmt = null;
		Connection conn = null;
		try
		{
			Class.forName("org.postgresql.Driver");
			System.out.println("Connecting to database...");
			conn = DriverManager.getConnection(DB_URL, user, pass);
			System.out.println("INSERT data...");
			stmt = conn.createStatement();
			
			String sql = "INSERT INTO observation(time, ep, object, instance, resource, value) " + 
			             "VALUES('" + time + "', '" + ep + "', " + obj + ", " + inst + ", " + res + ", " + val + ");";
			stmt.executeUpdate(sql);
			System.out.println("INSERT data successfully!");
			stmt.close();//finally block used to close resources
		
		}
		catch(SQLException se)
		{
			//Handle errors for JDBC
			se.printStackTrace();
		}
		catch(Exception e)
		{
			//Handle errors for Class.forName
			e.printStackTrace();
		}
	}

	public static void insert(String ep, int obj, int inst, int res, double val)
	{
		Statement stmt = null;
		Connection conn = null;
		try
		{
			Class.forName("org.postgresql.Driver");
			System.out.println("Connecting to database...");
			conn = DriverManager.getConnection(DB_URL, user, pass);
			System.out.println("INSERT data...");
			stmt = conn.createStatement();
			
			String sql = "INSERT INTO observation(ep, object, instance, resource, value) " + 
			             "VALUES('" + ep + "', " + obj + ", " + inst + ", " + res + ", " + val + ");";
			stmt.executeUpdate(sql);
			System.out.println("INSERT data successfully!");
			stmt.close();//finally block used to close resources
		
		}
		catch(SQLException se)
		{
			//Handle errors for JDBC
			se.printStackTrace();
		}
		catch(Exception e)
		{
			//Handle errors for Class.forName
			e.printStackTrace();
		}
	}


	public static void log(String ep, String ip, String regId)
	{
		Statement stmt = null;
		Connection conn = null;
		try
		{
			Class.forName("org.postgresql.Driver");
			System.out.println("Connecting to database...");
			conn = DriverManager.getConnection(DB_URL, user, pass);
			System.out.println("INSERT log...");
			stmt = conn.createStatement();
			
			String sql = "INSERT INTO log(ep, ip, regid) " + 
			             "VALUES('" + ep + "', '" + ip + "', '" + regId + "');";
			stmt.executeUpdate(sql);
			System.out.println("Logged successfully!");
			stmt.close();//finally block used to close resources
		
		}
		catch(SQLException se)
		{
			//Handle errors for JDBC
			se.printStackTrace();
		}
		catch(Exception e)
		{
			//Handle errors for Class.forName
			e.printStackTrace();
		}
	}

	public static void logError(String ep, String ip, String regId, String error)
	{
		Statement stmt = null;
		Connection conn = null;
		try
		{
			Class.forName("org.postgresql.Driver");
			System.out.println("Connecting to database...");
			conn = DriverManager.getConnection(DB_URL, user, pass);
			System.out.println("INSERT error into log...");
			stmt = conn.createStatement();
			
			String sql = "INSERT INTO log(ep, ip, regid, error) " + 
			             "VALUES('" + ep + "', '" + ip + "', '" + regId + "', '" + error + "');";
			stmt.executeUpdate(sql);
			System.out.println("Logged error successfully!");
			stmt.close();//finally block used to close resources
		
		}
		catch(SQLException se)
		{
			//Handle errors for JDBC
			se.printStackTrace();
		}
		catch(Exception e)
		{
			//Handle errors for Class.forName
			e.printStackTrace();
		}
	}
}
 