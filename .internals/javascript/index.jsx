import * as GUI from "GUI"
console.log("JSX test started...")
console.log("HELLO", globalThis)
console.log("Bye")

class MyComponent {
    layerX = -1;
    constructor(props) {
        this.props = props
        console.log("MyComponent constructor was called!")
    }

    anotherFunc() {
        return true
    }

    handleMouseMove = (event) => {
        console.log(event);
        this.layerX = event.layerX;
    }

    render() {
        // console.log("called render()")
        return (<vStack $backgroundColor="green" onMouseOver={this.handleMouseMove}>
            <text
                id="container"
                width={120}
                $letterSpacing={1}
                $padding={20}
                $backgroundColor="red">
                ... {this.layerX}
            </text>
        </vStack>);
    }
}

class Player {
    direction = {
        x: 1,
        y: 1,
    }
    constructor(x, y) {
        this.x = x;
        this.y = y;
    }

    move() {
        this.x += this.direction.x;
        this.y += this.direction.y;
    }
}

const player = new Player(0, 0);

const field = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
let grid = field.map((v, i) => <hStack>{field.map((v, j) => (<text $lineHeight={120} $padding={8} $backgroundColor={i === player.x && j === player.y ? "red" : "black"}> A </text>))}</hStack>)
console.log(grid)

// GUI.render((<vStack>
//     Hello

//     Bye

//     What
//     {grid}
// </vStack>))

const gameLoop = () => {
    player.move();
    grid = field.map((v, i) => <hStack>{field.map((v, j) => (<text $lineHeight={120} $padding={8} $backgroundColor={i === player.x && j === player.y ? "red" : "black"}> A </text>))}</hStack>)
    console.log("Moved");

    os.setTimeout(gameLoop, 500);
}

console.log(
    <MyComponent>
        This is another test
    </MyComponent>
)
