//STEP 1. Import required packages
import java.sql.*;

public class JDBCExample {
   // JDBC driver name and database URL
   static final String JDBC_DRIVER = "org.postgresql.Driver";  
   static final String DB_URL = "jdbc:postgresql://localhost:5432/airqdb";

   //  Database credentials
   static final String USER = "airq";
   static final String PASS = "airqpass";

   public static void insert(String ep, int obj, int inst, int res, double val) {
      Statement stmt = null;
      Connection conn = null;
      try {
         Class.forName("org.postgresql.Driver");
         System.out.println("Connecting to database...");
         conn = DriverManager.getConnection(DB_URL, USER, PASS);
         System.out.println("INSERT data...");
         stmt = conn.createStatement();
         
         String sql = "INSERT INTO log(ep, object, instance, resource, value) " + 
                      "VALUES('" + ep + "', " + obj + ", " + inst + ", " + res + ", " + val + ");";
         stmt.executeUpdate(sql);
         System.out.println("INSERT data successfully...");
         stmt.close();//finally block used to close resources
        
      } catch(SQLException se) {
         //Handle errors for JDBC
         se.printStackTrace();
      } catch(Exception e){
         //Handle errors for Class.forName
         e.printStackTrace();
      } finally {
      }//end finally try
   }
   
   public static void main(String[] args) {
      insert("airq", 3303, 0, 5700, 23.67);
      System.out.println("Goodbye!");//end main
   }
}//end JDBCExample
