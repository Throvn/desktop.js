import * as GUI from "GUI"
import * as os from "os"
// import * as React from "./build/libgui.so"
console.log("JSX test started...")
console.log("HELLO", globalThis)
console.log("Bye")
// const test = "hello";
// ===============

// class MyComponent extends React.Component {
class MyComponent {
    layerX = -1;
    constructor(props) {
        this.props = props
        print("MyComponent constructor was called!")
    }

    anotherFunc() {
        return true
    }

    handleMouseMove = (event) => {
        print(event);
        this.layerX = event.layerX;
    }

    render() {
        // print("called render()")
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

// os.setTimeout(changeColor, 30000)
const field = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
let grid = field.map((v, i) => <hStack>{field.map((v, j) => (<text $lineHeight={120} $padding={8} $backgroundColor={i === player.x && j === player.y ? "red" : "black"}> A </text>))}</hStack>)
print(grid)

GUI.render((<vStack>
    Hello

    Bye

    What
    {grid}
</vStack>))

const gameLoop = () => {
    player.move();
    grid = field.map((v, i) => <hStack>{field.map((v, j) => (<text $lineHeight={120} $padding={8} $backgroundColor={i === player.x && j === player.y ? "red" : "black"}> A </text>))}</hStack>)
    print("Moved");

    os.setTimeout(gameLoop, 500);
}

// gameLoop()
let evt = { layerX: -1 }
GUI.render(
    <MyComponent />
)

// print(test, JSON.stringify(new MyComponent().render()))