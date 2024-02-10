package pis;

import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;
import static org.junit.jupiter.api.Assertions.assertEquals;

public class MainTest {

    @Test
    void helloTest() {
        assertEquals("Hello world!", Main.getHello());
    }
}

@SpringBootTest
class MainTests {

    @Test
    void contextLoads() {
    }

}