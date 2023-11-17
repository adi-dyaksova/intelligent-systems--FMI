class Food {

    constructor(i, j) {
        this.i = i;
        this.j = j;
        this.x = blocks[i][j].x + blockSize / 2;
        this.y = blocks[i][j].y + blockSize / 2;
        this.diameter = blockSize / 1.5;
        this.isEaten = false;
    }

    draw() {
        fill(255, 0, 0);
        ellipse(this.x, this.y, this.diameter, this.diameter);
    }

    regenerate() {
        const possibleBlocks = [];
        for (var i = 0; i < blocks.length; i++) {
            for (var j = 0; j < blocks.length; j++) {
                if (!blocks[i][j].isUnplayable && !blocks[i][j].hasSnake)
                {
                    possibleBlocks.push([i, j]);
                }
            }
        }
        const randomBlock = Math.floor(Math.random() * Math.floor(possibleBlocks.length));
        [this.i, this.j] = possibleBlocks[randomBlock];
        this.x = blocks[this.i][this.j].y + blockSize / 2;
        this.y = blocks[this.i][this.j].x + blockSize / 2;
        this.isEaten = false;
    }
}
