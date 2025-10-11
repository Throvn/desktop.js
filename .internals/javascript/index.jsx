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

class GBText {
    constructor(props) {
        this.name = "SNAKE BOY"
        this.index = 0
        this.onMouseOver = props.onMouseOver || (() => {});
        this.color = "red";
        console.log("New GBText");

        this.interval = setInterval(() => {
            this.color = this.color === "red" ? "blue" : "red"
        }, 1000)
    }

    render() {
        return <group $color="#333960" $lineHeight={24 * 2} onMouseOver={this.onMouseOver}>
            <text>Throvn's</text>
            <text $fontSize={24} $color={this.color} onMouseDown={(e => {
                clearInterval(this.interval);
            })}>{this.name}</text>
            <text $fontSize={8}>TM</text>
        </group>;
    }
}

async function fetchImage() {
    console.log("fetching image")
    // const r = await fetch('https://raw.githubusercontent.com/raysan5/raylib/master/examples/models/models_loading.png');
    const r = await fetch('https://picsum.photos/200/300.jpg');
    // console.log(r)
    const b = await r.blob();
    // const b = new Blob([]);
    const blob = new Blob([b], {
        type: r.headers.get('content-type') ?? '',
    });
    console.log(blob.type);
    return blob;
}

class GameBoard {
    food = { x: 5, y: 5 };
    lastMousePos = { x: 0, y: 0 };
    tick = 0;
    lastUpdate = 0;

    constructor(props) {
        this.props = props;
        this.player = new Player(0, 0);
        this.hideText = false;

        setInterval(this.gameLoop, 500);
        fetchImage()
            .then(async b => {
                const arrayBuffer = await b.arrayBuffer();
                const buffer = new Uint8Array(arrayBuffer);
                // setTimeout(() => {
                this.imageData = b;
                // }, 3000);
            });
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
                        $lineHeight={3}
                        $padding={8}
                        $backgroundColor={isSnake ? "red" : isFood ? "green" : "#6f8500"}
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
                <vStack
                    $backgroundColor="#dbd8cc" 
                    $padding={8}
                    $borderRadius={{
                        top: 10,
                    }}
                    onMouseOver={this.handleMouseMove}
                    $width={350}
                >
                    <spacer />
                    <vStack
                    onKeyPress={(e) => {
                        console.log(e.key, e.code);
                    }}>
                    {"" + this.tick}
                        <hStack
                            $backgroundColor="#4d4a55" 
                            $padding={{
                                horizontal: 48,
                                vertical: 20,
                            }}
                            $borderRadius={{
                                top: 8 + this.tick,
                                bottomLeft: 8,
                                bottomRight: 32,
                            }}>
                            <vStack $borderRadius={10.5}>
                                {grid}
                            </vStack>
                        </hStack>
                        <hStack $gap={5} onMouseUp={() => {
                                this.hideText = !this.hideText;
                                console.log("hStack Mouse Up");
                            }}>
                            {!this.hideText ? <GBText/> : <text>Hidden</text>}
                            <spacer />
                            <img $height={100} data={this.imageData} onMouseUp={(e) => {
                                // e.stopPropagation();
                                console.log("Hidden Text Mouse Up");
                            }}>
                                <text $backgroundColor="green" $height={100}>This image is not yet rendered! Lorem ipsum dolor sit amet consectetur adipisicing elit. A sapiente quibusdam consequuntur quo explicabo vero blanditiis consectetur ratione laborum, sint dicta reprehenderit aliquam quia quod delectus illo perferendis ipsa debitis?</text>
                            </img>
                            {this.hideText ? 'Hideen' : <img $height={100} data={this.imageData} onMouseUp={(e) => {
                                // e.stopPropagation();
                                console.log("Hidden Text Mouse Up");
                            }} onMouseOver={(e) => {
                                this.hideText = true;
                                // e.stopPropagation();
                                const isEvent = e instanceof Event;
                                console.log("Image mouse over!", isEvent);
                            }}>
                                <text $backgroundColor="green" $height={100}>This image is not yet rendered! Lorem ipsum dolor sit amet consectetur adipisicing elit. A sapiente quibusdam consequuntur quo explicabo vero blanditiis consectetur ratione laborum, sint dicta reprehenderit aliquam quia quod delectus illo perferendis ipsa debitis?</text>
                            </img>}
                        </hStack>
                    </vStack>
                    <spacer />
                    <spacer />
                    <spacer />
                </vStack>
        );
    }
}
GUI.render(<GameBoard />, {
    width: 300,
    height: 500,
    resizable: false,
});
