
import java.io.IOException;
import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.SQLException;

import jakarta.servlet.annotation.WebServlet;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;

@WebServlet(value = "/delete_employee")

public class delete_employee extends HttpServlet {
    public void doGet(HttpServletRequest request, HttpServletResponse response) throws IOException {
        response.setContentType("text/html");
        PrintWriter out = response.getWriter();

        try {

            String employee_id = request.getParameter("employee_id");
            boolean result = new delete_employee().delete_emp("LDAP://192.168.1.8:389",employee_id);
            if (result) {
                out.print("<script>");
                out.print("alert(\"Deleted Successfully\");");
                out.print("window.location = \"http://localhost:8080/organization/view_employees.html\";");
                out.print("</script>");
                out.close();

            } else {
                out.print("<script>");
                out.print("alert(\"Error Occured\");");
                out.print("window.location = \"http://localhost:8080/organization/view_employees.html\";");
                out.print("</script>");
            }

        } catch (Exception e) {

            e.printStackTrace();

            out.print("<script>");
            out.print("alert(\"Error Occured\");");
            out.print("window.location = \"http://localhost:8080/organization/view_employees.html\";");
            out.print("</script>");

        }

    }

    static {
        System.loadLibrary("delete_emp");
    }

    private native boolean delete_emp(String ipaddress,String report_id);

}