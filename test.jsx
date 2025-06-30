import * as React from "./libgui.so"
console.log("JSX test started...")
// const test = <div>
//     Test
// </div>

// class MyComponent extends React.Component {
class MyComponent {
    constructor(props) {
        this.props = props
        print("MyComponent constructor was called!")
    }

    // anotherFunc() {
    //     return true
    // }

    // style() {
    //     return `
    //         * {
    //             background-color: black;
    //         }
    //     `
    // }

    render() {
        print("called render()")
        let a = "variable"
        return <h1 id="container" width={120}>
            <h2 id="nested">
                String
                <h3 id="super nested">
                    {a}
                </h3>
            </h2>
            <h4 id="same level as nested">
            </h4>
        </h1>;
    }
}

// React.render(<text>
//     <h1>Heading 1
//         <vStack>
//             <header>
//             </header>
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
//             <div>
//                 Second Div
//             </div>
//         </hStack>
//     </h2>
// </text>)
print("Pre render...")
React.render(<group $hello={"aaaa"} id="parentDiv">
    This is a test!
</group>)
print("After render...")
// setInterval(() => {
//     print("Interval")
// }, 3000)

// print(test, JSON.stringify(new MyComponent().render()))
