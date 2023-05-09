
import java.io.IOException;
import java.io.PrintWriter;
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;

import jakarta.servlet.annotation.WebServlet;
import jakarta.servlet.http.HttpServlet;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;

@WebServlet(value = "/view_employee")

public class view_employee extends HttpServlet {
    public void doGet(HttpServletRequest request, HttpServletResponse response) throws IOException {
        System.out.println(utils.getIpaddress());
        response.setContentType("application/json");
        PrintWriter out = response.getWriter();

        if (request.getParameter("option") != null) {
            if (request.getParameter("option").equalsIgnoreCase("selection")) {

                try {
                    String s = new view_employee().view_emp(utils.getIpaddress(),"all");
                    if (s != null && !s.equals("null")) {
                        out.write(s);
                    }
                    else{
                        response.setStatus(404);
                    }    

                } catch (Exception e) {
                    response.setStatus(404);
                    e.printStackTrace();
                }
            } else if (request.getParameter("option").equalsIgnoreCase("employees")) {
                 try {
                    String s = new view_employee().view_emp(utils.getIpaddress(),request.getParameter("report_id"));
                    if (s != null && !s.equals("null")) {
                        out.write(s);
                    }
                    else{
                        response.setStatus(404);
                    } 
                } catch (Exception e) {
                    response.setStatus(404);
                    e.printStackTrace();
                }
            }

        } else {
            try {
                System.out.println("start");
                String s = new view_employee().view_emp(utils.getIpaddress(),"all");

                System.out.println("data out");
                System.out.println(s);
                if (s != null && !s.equals("null")) {
                    response.getWriter().write(s);
                } else {
                    response.setStatus(404);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }

        }
    }

    

    

    static {
        System.loadLibrary("view_emp");
    }

    private native String view_emp(String ip_address,String report_id);

}

// JSONParser p = new JSONParser();
//                     JSONArray m_arr = (JSONArray) p.parse(s);
//                     JSONArray arr = new JSONArray();
//                     System.out.println("parsing......");
//                     for (int i = 0; i < m_arr.size(); i++) {
//                         JSONObject m_obj = (JSONObject) m_arr.get(i);
//                         JSONObject obj = new JSONObject();
//                         obj.put("employee_id", m_obj.get("employee_id"));
//                         obj.put("employee_name", m_obj.get("employee_name"));
//                         obj.put("phone_number", m_obj.get("phone_number"));
//                         obj.put("department_id", m_obj.get("department_id"));
//                         obj.put("report", m_obj.get("report"));

//                         if (m_obj.get("manager") == null) {
//                             obj.put("report", "-");
//                             obj.put("report_id", "-");

//                         } else {
//                             String manager[] = String.valueOf(m_obj.get("manager")).split("-");
                            
//                             obj.put("report_id", manager[1]);
//                         }

//                         if (m_obj.get("department").equals("-")) {
//                             obj.put("department", "-");
//                         } else {
//                             obj.put("department",
//                                     String.valueOf(m_obj.get("department")) + "-"
//                                             + getDepartmentName(String.valueOf(m_obj.get("department"))));
//                         }
//                         arr.add(obj);
//                     }
//                     System.out.println("response");
//                     response.getWriter().write(String.valueOf(arr));