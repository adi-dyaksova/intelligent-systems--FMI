var fieldSize = 20;
var playableAreaSize = 10;
var unplayableLimit = (fieldSize - playableAreaSize) / 2;

function setupBlocks() {
    for (var i = 0; i < fieldSize; i++) {
        blocks[i] = [];
        for (var j = 0; j < fieldSize; j++) {
            blocks[i][j] = new Block(i, j);
        }
    }
}

function setupUnplayableFields() {
    for (var i = 0; i < unplayableLimit; i++) {
        for (var j = 0; j < fieldSize; j++) {
            blocks[i][j].isUnplayable = true;
            blocks[fieldSize - i - 1][j].isUnplayable = true;
        }
    }

    for (var i = unplayableLimit; i < (fieldSize - unplayableLimit); i++) {
        for (var j = 0; j < unplayableLimit; j++) {
            blocks[i][j].isUnplayable = true;
            blocks[i][fieldSize - j - 1].isUnplayable = true;
        }
    }
}

function setupEdges() {
    for (var i = 1; i < blocks.length - 1; i++) {
        for (var j = 1; j < blocks[0].length - 1; j++) {
            if (blocks[i][j].isUnplayable) {
                if (!blocks[i + 1][j].isUnplayable) {
                    blocks[i][j].edges.push(1);
                }
                if (!blocks[i][j + 1].isUnplayable) {
                    blocks[i][j].edges.push(2);
                }
                if (!blocks[i - 1][j].isUnplayable) {
                    blocks[i][j].edges.push(3);
                }
                if (!blocks[i][j - 1].isUnplayable) {
                    blocks[i][j].edges.push(4);
                }
            }
        }
    }
}

function setupFood() {
    food = new Food(fieldSize / 2, fieldSize / 2);
}

function setupWalls() {
    walls.push(new Solid(blocks[unplayableLimit][unplayableLimit], blocks[unplayableLimit][fieldSize - unplayableLimit]));
    walls.push(new Solid(blocks[fieldSize - unplayableLimit][unplayableLimit], blocks[fieldSize - unplayableLimit][fieldSize - unplayableLimit]));
    walls.push(new Solid(blocks[unplayableLimit][unplayableLimit], blocks[fieldSize - unplayableLimit][unplayableLimit]));
    walls.push(new Solid(blocks[unplayableLimit][fieldSize - unplayableLimit], blocks[fieldSize - unplayableLimit][fieldSize - unplayableLimit]));
}
