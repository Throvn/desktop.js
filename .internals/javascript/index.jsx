import * as GUI from "GUI"

const SIZE = 10;
const MOUSE_THRESHOLD = 5; // pixels needed to trigger direction change

class Player {
    direction = { x: 1, y: 0 };
    body = [];
    constructor(x, y) {
        this.body = [{ x, y }];
    }

    move() {
        const head = {
            x: (this.body[0].x + this.direction.x + SIZE) % SIZE,
            y: (this.body[0].y + this.direction.y + SIZE) % SIZE
        };
        this.body.unshift(head);
        this.body.pop();
    }

    grow() {
        const tail = this.body[this.body.length - 1];
        this.body.push({ ...tail });
    }
}

class GameBoard {
    food = { x: 5, y: 5 };
    lastMousePos = { x: 0, y: 0 };
    tick = 0;
    lastUpdate = 0;

    constructor(props) {
        this.props = props;
        this.player = new Player(0, 0);

        setInterval(this.gameLoop, 500);
    }

    gameLoop = () => {
        this.tick++;

        // Move snake
        this.player.move();

        // Self collision check
        const head = this.player.body[0];
        if (this.player.body.slice(1).some(s => s.x === head.x && s.y === head.y)) {
            console.log("Game Over");
            this.player = new Player(0, 0);
        }

        // Eat food
        if (this.isEating()) {
            this.player.grow();
            this.spawnFood();
        }
    }

    handleMouseMove = (event) => {
        const mouseX = event.layerX;
        const mouseY = event.layerY;

        const dx = mouseX - this.lastMousePos.x;
        const dy = mouseY - this.lastMousePos.y;

        // Only update if movement exceeds threshold
        if (Math.abs(dx) >= MOUSE_THRESHOLD || Math.abs(dy) >= MOUSE_THRESHOLD) {
            if (Math.abs(dx) > Math.abs(dy)) {
                this.player.direction = { x: dx > 0 ? 1 : -1, y: 0 };
            } else if (dy !== 0) {
                this.player.direction = { x: 0, y: dy > 0 ? 1 : -1 };
            }
            this.lastMousePos = { x: mouseX, y: mouseY };
        }
    }

    spawnFood() {
        let pos;
        do {
            pos = { x: Math.floor(Math.random() * SIZE), y: Math.floor(Math.random() * SIZE) };
        } while (this.player.body.some(s => s.x === pos.x && s.y === pos.y));
        this.food = pos;
    }

    isEating() {
        const head = this.player.body[0];
        return head.x === this.food.x && head.y === this.food.y;
    }

    render() {
        // Draw grid
        const grid = [];
        for (let i = 0; i < SIZE; i++) {
            const row = [];
            for (let j = 0; j < SIZE; j++) {
                const isSnake = this.player.body.some(s => s.x === j && s.y === i);
                const isFood = this.food.x === j && this.food.y === i;
                row.push(
                    <text
                        $lineHeight={120}
                        $padding={8}
                        $backgroundColor={isSnake ? "red" : isFood ? "green" : "black"}
                    $borderRadius={{
                        bottomLeft: 20
                    }}
                    >
                        {" "}
                    </text>
                );
            }
            grid.push(<hStack $borderRadius={20}>{row}</hStack>);
        }

        return (
            <hStack $backgroundColor="green" onMouseOver={this.handleMouseMove} $borderRadius={{
                top: 50,
            }}>
                <spacer />
                <vStack $backgroundColor="blue" $padding={8} $borderRadius={{
                    // top: 16,
                    bottomLeft: 8,
                    bottomRight: 32,
                }}>
                    <spacer />
                    {"" + this.tick}
                    <vStack $borderRadius={10.5}>
                        {grid}
                    </vStack>
                    <spacer />
                </vStack>
                <spacer />
            </hStack>
        );
    }
}

GUI.render(<GameBoard />)
