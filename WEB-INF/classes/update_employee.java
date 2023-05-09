
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
@WebServlet(value = "/update_employee")

public class update_employee extends HttpServlet {
    public void doGet(HttpServletRequest request, HttpServletResponse response) throws IOException {
        response.setContentType("text/html");
        PrintWriter out = response.getWriter();

        try {
            String name=request.getParameter("name");
            String employee_id=request.getParameter("employee_id");
            String phone_number=request.getParameter("phone_number");
            String department_id=request.getParameter("department_id");
            String report=request.getParameter("report");
            boolean result=new update_employee().update_emp(utils.getIpaddress(),employee_id,name,phone_number,department_id,report);
            if(result){
                out.print("<script>");
            out.print("alert(\"Updated Successfully\");");
            out.print("window.location = \"http://localhost:8080/organization/view_employees.html\";");
            out.print("</script>");
            out.close();
            }
            else{
                throw new Exception();
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
        System.loadLibrary("update_emp");
    }
    private native boolean update_emp(String ipaddress,String employee_id,String employee_name,String phone_number,String department_id,String report);

}