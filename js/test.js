import * as React from "./build/libgui.so";
console.log("JSX test started...");
var test = React.createElement("div", null, "Test1");
// class MyComponent extends React.Component {
var MyComponent = /** @class */ (function () {
    function MyComponent(props) {
        this.props = props;
        print("MyComponent constructor was called!");
    }
    MyComponent.prototype.anotherFunc = function () {
        return true;
    };
    MyComponent.prototype.style = function () {
        return "\n            * {\n                background-color: black;\n            }\n        ";
    };
    MyComponent.prototype.render = function () {
        print("called render()");
        var a = "variable how fucking cool is that?";
        return React.createElement("h1", { id: "container", width: 120 },
            React.createElement("h2", { id: "nested" },
                "String",
                React.createElement("h3", { id: "super nested" }, a)),
            React.createElement("h4", { id: "same level as nested" }));
    };
    return MyComponent;
}());
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
print("Pre render...");
React.render(React.createElement("group", { "$backgroundColor": "#00af00f0" },
    React.createElement("hStack", null,
        React.createElement("group", { "$backgroundColor": "#0000ff6f" },
            React.createElement("text", null, "Cool String"),
            React.createElement("text", null, "Another String"),
            React.createElement("spacer", null),
            "Yeah!!!!!!!!!!"),
        React.createElement("spacer", null)),
    React.createElement("vStack", { "$backgroundColor": "green", "$hello": "aaaa", id: "parentDiv" },
        "This is a test!",
        React.createElement("spacer", null),
        React.createElement(MyComponent, null),
        "This is a test!",
        React.createElement("spacer", null),
        "This is a test!",
        React.createElement("spacer", null),
        "This is a test!",
        React.createElement("spacer", null),
        "This is a test!",
        React.createElement("spacer", null),
        "This is a test!",
        React.createElement("spacer", null),
        "This is a test!")));
print("After render...");
// setInterval(() => {
//     print("Interval")
// }, 3000)
// print(test, JSON.stringify(new MyComponent().render()))
