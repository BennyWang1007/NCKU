import java.math.BigInteger;
import java.util.Scanner;

public class HW2 {
    public static void main(String[] args) {
        Scanner sc = new Scanner(System.in);
        int testCases = Integer.parseInt(sc.nextLine());

        for (int i = 0; i < testCases; i++) {
            String[] input = sc.nextLine().split(" ");
            String[] num1 = input[0].split("\\."); // num1[0] is the integer part, num1[1] is the decimal part
            String[] num2 = input[1].split("\\."); // num2[0] is the integer part, num2[1] is the decimal part

            // case when there is no decimal part
            if (num1.length == 1) num1 = new String[] {num1[0], "0"};
            if (num2.length == 1) num2 = new String[] {num2[0], "0"};

            // case when there is no integer part
            if (num1[0].isEmpty()) num1[0] = "0";
            if (num2[0].isEmpty()) num2[0] = "0";

            int len1 = num1[1].length();
            int len2 = num2[1].length();
            int len = lcm(len1, len2);

            String decimal1 = num1[1].repeat(len / len1);
            String decimal2 = num2[1].repeat(len / len2);

            BigInteger resultDecimal = new BigInteger(decimal1).add(new BigInteger(decimal2));
            BigInteger resultInteger = new BigInteger(num1[0]).add(new BigInteger(num2[0]));

            // handling carry over
            if (resultDecimal.toString().length() > len) {
                resultDecimal = resultDecimal.add(BigInteger.ONE).subtract(BigInteger.TEN.pow(len));
                resultInteger = resultInteger.add(BigInteger.ONE);
            }

            String resultDecimalStr = formatDecimalString(resultDecimal.toString(), len);
            
            // handling 0.9999... case
            if (resultDecimalStr.equals("9")) {
                resultInteger = resultInteger.add(BigInteger.ONE);
                resultDecimalStr = "0";
            }

            printResult(resultInteger, resultDecimalStr);
        }

        sc.close();
    }

    private static int lcm(int a, int b) {
        return a * b / gcd(a, b);
    }

    private static int gcd(int a, int b) {
        return (b == 0) ? a : gcd(b, a % b);
    }

    /**
     * add missing zeros to the front, and remove the repeated part 
     */
    private static String formatDecimalString(String decimal, int len) {

        if (decimal.length() < len) {
            decimal = "0".repeat(len - decimal.length()) + decimal;
        }
        String new_str = getShortestRepeat(decimal);
        while (!new_str.equals(decimal)) {
            decimal = new_str;
            new_str = getShortestRepeat(decimal);
        }
        return decimal;
    }


    /**
     * get the shortest repeat number
     * e.g. 0.123123123123, the repeat number is 123, not 123123
     */
    private static String getShortestRepeat(String decimal) {
        
        int len = decimal.length();
        for (int i = len / 2; i > 0; i--) {
            if (len % i == 0) {
                String subStr = decimal.substring(0, i);
                String repeatStr = subStr.repeat(len / i);
                if (repeatStr.equals(decimal)) {
                    return subStr;
                }
            }
        }
        return decimal;
    }

    private static void printResult(BigInteger resultInteger, String resultDecimalStr) {
        System.out.print(resultInteger);
        if (!resultDecimalStr.isEmpty() && !resultDecimalStr.equals("0")) {
            System.out.print("." + resultDecimalStr);
        }
        System.out.println();
    }
}
