//PriorityQueue
{const top=0,parent=c=>(c+1>>>1)-1,left=c=>(c<<1)+1,right=c=>c+1<<1;class PriorityQueue{constructor(c=(d,e)=>d>e){this._heap=[],this._comparator=c}size(){return this._heap.length}isEmpty(){return 0==this.size()}peek(){return this._heap[top]}push(...c){return c.forEach(d=>{this._heap.push(d),this._siftUp()}),this.size()}pop(){const c=this.peek(),d=this.size()-1;return d>top&&this._swap(top,d),this._heap.pop(),this._siftDown(),c}replace(c){const d=this.peek();return this._heap[top]=c,this._siftDown(),d}_greater(c,d){return this._comparator(this._heap[c],this._heap[d])}_swap(c,d){[this._heap[c],this._heap[d]]=[this._heap[d],this._heap[c]]}_siftUp(){for(let c=this.size()-1;c>top&&this._greater(c,parent(c));)this._swap(c,parent(c)),c=parent(c)}_siftDown(){for(let d,c=top;left(c)<this.size()&&this._greater(left(c),c)||right(c)<this.size()&&this._greater(right(c),c);)d=right(c)<this.size()&&this._greater(right(c),left(c))?right(c):left(c),this._swap(c,d),c=d}}window.PriorityQueue=PriorityQueue}

class Snake {
    constructor() {
        this.size = 3;
        this.blocks = [];
        for (var i = 0; i < this.size; i++)
        {
            this.blocks[i] = [unplayableLimit + 2, unplayableLimit + 3 - i];
            blocks[unplayableLimit + 2][unplayableLimit + 3 - i].hasSnake = true;
        }
        this.speed = blockSize / 21.0;
        this.reachedGoal = false;
        this.isDead = false;
        this.directionsToFood = this.findPath();
        this.diameter = blockSize / 2.5;
        this.isDead = false;
    }

    start() {
        setInterval(() => {
            this.move();
            draw2();
        }, 100);
    }

    draw() {
        for (var k = 0; k < this.blocks.length; k++) {
            const [i, j] = this.blocks[k]
            fill(0, 0, 255);
            rect(blocks[i][j].y, blocks[i][j].x, blockSize, blockSize);
            if (k == 0)
            {   
                const fX = blocks[i][j].x + blockSize / 2;
                const fY = blocks[i][j].y + blockSize / 2;
                fill(0, 0, 150);
                ellipse(fY, fX, this.diameter, this.diameter);
            }
        }
    }

    async move() {
        let direction = this.directionsToFood.pop();
        if (!direction) {
            this.directionsToFood = this.findPath(); 
            direction = this.directionsToFood.pop()
        }

        const [firstBlockI, firstBlockJ] = this.blocks[0];
        const nextBlock = [firstBlockI + direction[0], firstBlockJ + direction[1]];
        
        blocks[nextBlock[0]][nextBlock[1]].hasSnake = true;
        this.blocks.unshift(nextBlock);
        if (nextBlock[0] == food.i && nextBlock[1] == food.j)
        {
            food.isEaten = true;
            food.regenerate();
        } else {
            const last = this.blocks.pop();
            blocks[last[0]][last[1]].hasSnake = false;
        }
       
        
        if (blocks[nextBlock[0]][nextBlock[1]].isUnplayable) {
            this.isDead = true;
        }
    }

    findPath() {
        const queue = new PriorityQueue((a, b) => a[0].cost() + a.length < b[0].cost() + b.length);
        let bestState = [new Path([...this.blocks])];

        while (bestState && bestState[0].cost() !== 0) { 
            const extendedPaths = bestState[0].extend();
            extendedPaths.map(path => [path, ...bestState]).forEach(state => queue.push(state));
            bestState = queue.pop();
        }

        const restore = []; 
        if(bestState){
            for (let i=0; i < bestState.length - 1; i++) {
                restore[i] = [bestState[i].path[0][0] - bestState[i+1].path[0][0], bestState[i].path[0][1] - bestState[i+1].path[0][1]];
            }
        }
        
        return restore;
    }

}


class Path {
    constructor(path) {
        this.path = path;
    }

    extend() {
        const directions = [
            { 'x': 0, 'y': -1 },
            { 'x': 1, 'y': 0 },
            { 'x': 0, 'y': 1 },
            { 'x': -1, 'y': 0 }
        ];

        const [lastBlockX, lastBlockY] = this.path[0];

        this.path.pop();

        return directions
        .map(direction => [lastBlockX + direction.x, lastBlockY + direction.y])
        .filter(nextBlock => !blocks[nextBlock[0]][nextBlock[1]].isUnplayable)
        .filter(nextBlock => {
            return this.isNotInPath(nextBlock);
        })
        .map(nextBlock => new Path([nextBlock, ...this.path]));
    }

    isNotInPath(block) {
        return !(this.path.find(el => el[0] === block[0] && el[1] === block[1]));
    }

    cost() {
        const [lastBlockX, lastBlockY] = this.path[0];
        return Math.abs(lastBlockX - food.i) + Math.abs(lastBlockY - food.j);
    }
}