import jakarta.servlet.*;
import jakarta.servlet.http.HttpServletRequest;
import jakarta.servlet.http.HttpServletResponse;
import java.io.IOException;

public class CorsFilter implements Filter {

    @Override
    public void doFilter(ServletRequest req, ServletResponse res, FilterChain chain) throws IOException, ServletException {
        HttpServletResponse response = (HttpServletResponse) res;

        // Allow requests from localhost:4200
        response.setHeader("Access-Control-Allow-Origin", "http://localhost:4200");

        // Allow the following HTTP methods
        response.setHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE");

        // Allow the following HTTP headers
        response.setHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");

        // Allow credentials to be sent (e.g. cookies)
        response.setHeader("Access-Control-Allow-Credentials", "true");

        chain.doFilter(req, res);
    }

    @Override
    public void init(FilterConfig config) throws ServletException {
        // Initialization code, if needed
    }

    @Override
    public void destroy() {
        // Clean-up code, if needed
    }
}
