package pis.apigateway;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.cloud.gateway.route.RouteDefinition;
import org.springframework.cloud.gateway.route.RouteDefinitionLocator;
import org.springframework.context.annotation.Bean;

@SpringBootApplication
@EnableDiscoveryClient
public class ApiGatewayApplication {

	public static void main(String[] args) {
		SpringApplication.run(ApiGatewayApplication.class, args);
	}

//	@Bean
//	public RouteLocator routeLocator(RouteLocatorBuilder builder) {
//		return builder.routes()
//				.route("elastic-search-route", r -> r.path("/search/**").uri("lb://elastic-search-service"))
//				.route("images-route", r -> r.path("/image/**").uri("lb://images-service"))
//				.route("recipe-route", r -> r.path("/recipe/**").uri("lb://recipe-service"))
//				.route("user-route", r -> r.path("/user/**").uri("lb://user-service"))
//				.build();
//	}
}