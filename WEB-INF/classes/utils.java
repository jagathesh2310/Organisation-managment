import java.io.FileNotFoundException;
import java.io.FileReader;
import java.util.Properties;

public class utils {
    public static String getIpaddress(){
        FileReader reader;
        try {
            reader = new FileReader("C:\\Program Files\\Apache Software Foundation\\Tomcat 10.1\\webapps\\organization\\data\\ip.properties");
            Properties p = new Properties();
            p.load(reader);
            return p.getProperty("ip_address");
        } catch (Exception e) {
            e.printStackTrace();
        }
        return "";
            
    }

}
