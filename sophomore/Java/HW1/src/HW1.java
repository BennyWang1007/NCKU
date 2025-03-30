import java.util.Scanner;

public class HW1 {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        while (sc.hasNextLine()) {
            String line = sc.nextLine();
            if (line.equals("\n") || line.equals("")) {
                break;
            }
            String num1 = line.split(" ")[0];
            String num2 = line.split(" ")[1];
            // add two big float to string that preserve the precision
            String result = new java.math.BigDecimal(num1).add(new java.math.BigDecimal(num2)).toString();
            System.out.println(result);
        }
        sc.close();
    }
}