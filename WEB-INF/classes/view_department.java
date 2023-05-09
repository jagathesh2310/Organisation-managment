
import java.io.IOException;
import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;

import jakarta.servlet.annotation.WebServlet;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
@WebServlet(value = "/view_department")

public class view_department extends HttpServlet {
    public void doGet(HttpServletRequest request, HttpServletResponse response) throws IOException {
        response.setContentType("application/json");
        PrintWriter out = response.getWriter();

        try {
            Class.forName("org.postgresql.Driver");
            Connection c = DriverManager.getConnection("jdbc:postgresql://localhost:5432/organization", "postgres",
                    "admin");
            PreparedStatement p = c.prepareStatement("select * from department;");
            ResultSet r=p.executeQuery();
            JSONArray arr=new JSONArray();
            while(r.next()){
                JSONObject obj=new JSONObject();
                obj.put("department_name", r.getString("department_name"));
                obj.put("department_id", r.getString("department_id"));
                arr.add(obj);
            }
            c.close();
            out.write(String.valueOf(arr));

            

        } catch (Exception e) {
            response.setStatus(404);
            e.printStackTrace();
        }

    }

    public void doPost(HttpServletRequest request, HttpServletResponse response) throws IOException {
        response.setContentType("text/html");
        PrintWriter out = response.getWriter();

        if(request.getParameter("dept_id").equals("-")){
            out.write("-");
        }
        else{
            try {
                Class.forName("org.postgresql.Driver");
                Connection c = DriverManager.getConnection("jdbc:postgresql://localhost:5432/organization", "postgres",
                        "admin");
                PreparedStatement p = c.prepareStatement("select * from department where department_id=?;");
                p.setString(1,request.getParameter("dept_id"));
                ResultSet r=p.executeQuery();
                String s="";
                if(r.next()){
                    s+= r.getString("department_name")+"-"+r.getString("department_id");
    
                }
                c.close();
                out.write(s);
    
                
    
            } catch (Exception e) {
                response.setStatus(404);
                e.printStackTrace();
            }
        }

        

    }

}