package pis.configsever;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.cloud.config.server.EnableConfigServer;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;

@SpringBootApplication
@EnableConfigServer
@EnableDiscoveryClient
public class ConfigSeverApplication {

	public static void main(String[] args) {
		SpringApplication.run(ConfigSeverApplication.class, args);
	}

}
