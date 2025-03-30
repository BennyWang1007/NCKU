import java.util.ArrayList;
import java.util.Scanner;

public class Solution1 {
    public static void main(String[] args) {
        
        Scanner scanner = new Scanner(System.in);
        int n = scanner.nextInt();
        scanner.nextLine();
        for (int j = 0; j < n; j++) {
            String line = scanner.nextLine();
            ArrayList<Character> newLine = new ArrayList<>();
            // add space at both side of ops and x's
            for(char c : line.toCharArray()) {
                if (c == 'x' || c == '+' || c == '-') {
                    newLine.add(' ');
                    newLine.add(c);
                    newLine.add(' ');
                } else {
                    newLine.add(c);
                }
            }
            line = "";
            for (char c : newLine) {
                line += c;
            }

            String[] pairs = line.split("=");
            int a = 0, b = 0;
            int a1 = 0, b1 = 0; // ax+b = a1 x+b1

            int[] results = new int[2];

            readEq(pairs[0], results);
            a = results[0];
            b = results[1];

            readEq(pairs[1], results);
            a1 = results[0];
            b1 = results[1];

            a -= a1;
            b1 -= b; // (a-a1)x = (b1-b)
            if (a == 0) {
                if (b1 == 0) System.out.println("Infinite solutions");
                else System.out.println("No solution");
            } else {
                System.out.println("x="+b1/a);
            }
        }
        scanner.close();
    }

    public static void readEq(String s, int results[]) {
        Scanner sc = new Scanner(s);
        int a = 0, b = 0;
        int start_idx = 0;

        ArrayList<String> firstList = new ArrayList<>();
        
        while(sc.hasNext()){
            firstList.add(sc.next());
        }
        sc.close();

        // for (String tmp : firstList) {
        //     System.out.println("|" + tmp + "|");
        // }

        if (!firstList.get(0).equals("-")) {
            firstList.add(0, "+");
        }

        int num = 0;
        for (int i = start_idx; i + 1 < firstList.size();) {
            String numStr = firstList.get(i+1);
            boolean isPos = firstList.get(i).equals("+");
            i += 2;
            // case +x/-x
            if (numStr.equals("x")) {
                if (isPos)  a += 1;
                else a -= 1;
                continue;
            }
            num = Integer.parseInt(numStr);
            // case +-ax
            if (i < firstList.size() && firstList.get(i).equals("x")) {
                if (isPos) a += num;
                else a -= num;
                i += 1;
            } else { //case +-b
                if (isPos) b += num;
                else b -= num;
            }
        }
        results[0] = a;
        results[1] = b;
        
    }

}
