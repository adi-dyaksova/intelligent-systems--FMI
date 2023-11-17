class Block {
    constructor(x, y) {
        this.x = x * blockSize;
        this.y = y * blockSize;
        this.isUnplayable = false;
        this.edges = [];
        this.color = null;
        this.defaultColor = (x + y) % 2 == 0 ? 'rgb(77, 219, 55)' : 'rgb(90, 250, 65)';
        this.hasSnake = false;
    }

    draw() {
        fill(this.color || this.defaultColor);
        if (this.isUnplayable) {
            fill(50, 143, 13);
        }
        noStroke();
        rect(this.x, this.y, blockSize, blockSize);
    }

    drawEdges() {
        for (var i = 0; i < this.edges.length; i++) {
            stroke(0);
            strokeWeight(4);
            switch (this.edges[i]) {
                case 4:
                    line(this.x, this.y, this.x + blockSize, this.y);
                    break;
                case 1:
                    line(this.x + blockSize, this.y, this.x + blockSize, this.y + blockSize);
                    break;
                case 2:
                    line(this.x, this.y + blockSize, this.x + blockSize, this.y + blockSize);
                    break;
                case 3:
                    line(this.x, this.y, this.x, this.y + blockSize);
                    break;
            }
        }
    }



}
