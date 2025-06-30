import * as React from "./libgui.so";
console.log("JSX test started...");
// const test = <div>
//     Test
// </div>
// class MyComponent extends React.Component {
var MyComponent = /** @class */ (function () {
    function MyComponent(props) {
        this.props = props;
        print("MyComponent constructor was called!");
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
    MyComponent.prototype.render = function () {
        print("called render()");
        var a = "variable";
        return React.createElement("h1", { id: "container", width: 120 },
            React.createElement("h2", { id: "nested" },
                "String",
                React.createElement("h3", { id: "super nested" }, a)),
            React.createElement("h4", { id: "same level as nested" }));
    };
    return MyComponent;
}());
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
print("Pre render...");
React.render(React.createElement("vStack", { "$backgroundColor": "gibberish", "$hello": "aaaa", id: "parentDiv" },
    React.createElement("spacer", null),
    "This is a test!",
    React.createElement("spacer", null)));
print("After render...");
// setInterval(() => {
//     print("Interval")
// }, 3000)
// print(test, JSON.stringify(new MyComponent().render()))
