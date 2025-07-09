import * as GUI from "GUI"
console.log(GUI)
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
        return <text id="container" width={120}>
            String
            {a}
        </text>;
    }
}
GUI.render(<group $backgroundColor="#000000f0">
    <hStack $padding={10}>
        <group
            $backgroundColor="#0000ff6f"
            $padding={10}
            $fontSize={30}
            $color="tomato">
            <text $backgroundColor="#00ff00">About Me</text>
            <spacer />
            <text $fontSize={20} $color="#ff00ff">Another String</text>
            <spacer />
            Go and hire me!
        </group>
    </hStack>
    <vStack
        $backgroundColor="green"
        $padding={{ horizontal: 100 }}
        $hello={"aaaa"}
        $color="blue"
        id="parentDiv">
        <spacer />
        This is a test!
        <spacer />
        <MyComponent $padding={{ left: 12 }} />
        This is a test!
        <spacer />
    </vStack>
</group>)
setInterval(() => {
    print("After render...")
    print("Interval")
}, 3000)

// print(test, JSON.stringify(new MyComponent().render()))
