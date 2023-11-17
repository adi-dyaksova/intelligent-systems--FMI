class Solid {

    constructor(fromBlock, toBlock) {
        const lineWidth = 1;
        this.x = fromBlock.x - lineWidth;
        this.y = fromBlock.y - lineWidth;
        this.w = toBlock.x + blockSize - this.x + lineWidth;
        this.h = toBlock.y + blockSize - this.y + lineWidth;
        this.bottomRightX = this.x + this.w;
        this.bottomRightY = this.y + this.h;

    }
}
