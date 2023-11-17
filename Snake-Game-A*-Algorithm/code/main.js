const blockSize = 25;
const blocks = [];
const walls = [];
let food = null;
let snake = null;

function setup() {
    createCanvas(1280, 1280);
    setupLevel();
}

function draw2() {
    drawBlocks();
    drawSnake();
    drawFood();
}

function drawBlocks() {
    for (var i = 0; i < blocks.length; i++) {
        for (var j = 0; j < blocks[i].length; j++) {
            blocks[i][j].draw();
        }
    }
    for (var i = 0; i < blocks.length; i++) {
        for (var j = 0; j < blocks[i].length; j++) {
            blocks[i][j].drawEdges();
        }
    }
}

function regenerateFood() {
    if(food.isEaten) {
        food.regenerate();
    }
}

function drawFood() {
    food.draw();
}

function moveSnake() {
    snake.move();
}

function drawSnake() {
    snake.draw();
}

function setupLevel() {
    setupBlocks();
    setupUnplayableFields();
    setupEdges();
    setupWalls();
    setupFood();
    snake = new Snake();
    draw2();
    snake.start();
}
