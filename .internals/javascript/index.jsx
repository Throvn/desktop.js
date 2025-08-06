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
    constructor(props) {
        this.props = props
        print("MyComponent constructor was called!")
    }

    anotherFunc() {
        return true
    }

    render() {
        print("called render()")
        let a = "variable how fucking cool is that?"
        return <text id="container" width={120} $letterSpacing={10}>
            String
            {a}
        </text>;
    }
}

// os.setTimeout(changeColor, 30000)
const field = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
const grid = field.map((v, i) => <hStack>{field.map((v, j) => (<vStack $backgroundColor="red">A {i + j}</vStack>))}</hStack>)
print(grid)

// print(test, JSON.stringify(new MyComponent().render()))
GUI.render((<vStack>
    Hello

    Bye

    What
    {grid}
</vStack>))
