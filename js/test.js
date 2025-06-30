import * as React from "./build/libgui.so";
console.log("JSX test started...");
// const test = "hello";
// ===============
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
        return React.createElement("text", { id: "container", width: 120 },
            "String",
            a);
    };
    return MyComponent;
}());
React.render(React.createElement("group", { "$backgroundColor": "#00af00f0" },
    React.createElement("hStack", { "$padding": 10 },
        React.createElement("group", { "$backgroundColor": "#0000ff6f", "$padding": 10 },
            React.createElement("text", { "$backgroundColor": "#00ff00" }, "About Me"),
            React.createElement("spacer", null),
            React.createElement("text", null, "Another String"),
            React.createElement("spacer", null),
            "Yeah!!!!!!!!!!")),
    React.createElement("vStack", { "$backgroundColor": "green", "$padding": { horizontal: 100 }, "$hello": "aaaa", id: "parentDiv" },
        React.createElement("spacer", null),
        "This is a test!:::::::::::::::::::::::::::::::::::::::::::::::::::::::",
        React.createElement("spacer", null),
        React.createElement(MyComponent, { "$padding": { left: 12 } }),
        "This is a test!",
        React.createElement("spacer", null))));
setInterval(function () {
    print("After render...");
    //     print("Interval")
    // }, 3000)
    // print(test, JSON.stringify(new MyComponent().render()))
});
//     print("Interval")
// }, 3000)
// print(test, JSON.stringify(new MyComponent().render()))
