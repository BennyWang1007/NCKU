public class Point {
    private int x;
    private int y;

    public Point() {
        this.x = 0;
        this.y = 0;
    }

    public int RetrieveVertical() {
        return this.y;
    }

    public int RetrieveHorizontal() {
        return this.x;
    }

    // Rotate 90
    public void Rotate() {
        int temp = this.x;
        this.x = this.y;
        this.y = -temp;
    }

    public void Move(int x, int y) {
        this.x += x;
        this.y += y;
    }

    public void Set(int x, int y) {
        this.x = x;
        this.y = y;
    }

    public int calculateManhattanDistance(Point other) {
        return Math.abs(this.x - other.x) + Math.abs(this.y - other.y);
    }

    public double ChebyshevDistance(Point other) {
        return Math.max(Math.abs(this.x - other.x), Math.abs(this.y - other.y));
    }

    public void PrintPosition() {
        System.out.println(this.x + " " + this.y);
    }
}
