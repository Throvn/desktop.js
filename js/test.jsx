import * as React from "./build/libgui.so"
console.log("JSX test started...")
const test = <div>
    Test1
</div>

// class MyComponent extends React.Component {
class MyComponent {
    constructor(props) {
        this.props = props
        print("MyComponent constructor was called!")
    }

    anotherFunc() {
        return true
    }

    style() {
        return `
            * {
                background-color: black;
            }
        `
    }

    render() {
        print("called render()")
        let a = "variable how fucking cool is that?"
        return <h1 id="container" width={120}>
            <h2 id="nested">
                String
                <text padding={16} id="super nested">
                    {a}
                </text>
            </h2>
            <h4 id="same level as nested">
            </h4>
        </h1>;
    }
}

// React.render(<vStack>
//     <h1>Heading 1
//         <vStack>
//             Test..
//             <footer>
//             </footer>
//         </vStack>
//     </h1>
//     <h2>
//         Heading 2
//         <hStack>
//             <div
//                 onClick={() => console.log("Cool!")}
//                 width={300}
//                 height={400}
//                 borderRadius={25}
//                 color="red"
//             >
//                 Other
//             </div>
//             <spacer />
//             <div>
//                 Second Div
//             </div>
//         </hStack>
//     </h2>
// </vStack>)
print("Pre render...")
React.render(<group $backgroundColor="#00af00f0">
    <hStack>
        <group $backgroundColor="#0000ff6f" $padding={10}>
            <text $backgroundColor="#00ff00">Cool String</text>
            <text>Another String</text>
            <spacer />
            Yeah!!!!!!!!!!
        </group>
        <spacer />
    </hStack>
    <vStack
        $backgroundColor="green"
        $hello={"aaaa"}
        id="parentDiv">
        This is a test!
        <spacer />
        <MyComponent />
        This is a test!
        <spacer />
        This is a test!
        <spacer />
        This is a test!
        <spacer />
        This is a test!
        <spacer />
        This is a test!
        <spacer />
        This is a test!
    </vStack>
</group>)
print("After render...")
// setInterval(() => {
//     print("Interval")
// }, 3000)

// print(test, JSON.stringify(new MyComponent().render()))
