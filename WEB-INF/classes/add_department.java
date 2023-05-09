
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
@WebServlet(value = "/add_department")

public class add_department extends HttpServlet {
    public void doPost(HttpServletRequest request, HttpServletResponse response) throws IOException {
        response.setContentType("text/html");
        PrintWriter out = response.getWriter();

        try {
            Class.forName("org.postgresql.Driver");
            Connection c = DriverManager.getConnection("jdbc:postgresql://localhost:5432/organization", "postgres",
                    "admin");
            PreparedStatement p = c.prepareStatement("insert into department values(?,?);");
            p.setString(1, request.getParameter("department_name"));
            p.setString(2, request.getParameter("department_id"));
            p.execute();
            c.close();

            out.print("<script>");
            out.print("alert(\"Inserted\");");
            out.print("window.location = \"http://localhost:8080/organization\";");
            out.print("</script>");
            out.close();
          

        } catch (Exception e) {
            e.printStackTrace();

            out.print("<script>");
            out.print("alert(\"Already Exists\");");
            out.print("window.location = \"http://localhost:8080/organization\";");
            out.print("</script>");
            
        }

    }

}