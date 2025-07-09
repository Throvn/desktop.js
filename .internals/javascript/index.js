import * as GUI from "GUI";
console.log(GUI);
// import * as React from "./build/libgui.so"
console.log("JSX test started...");
console.log("HELLO", globalThis);
console.log("Bye");
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
    MyComponent.prototype.render = function () {
        print("called render()");
        var a = "variable how fucking cool is that?";
        return GUI.createElement("text", { id: "container", width: 120 },
            "String",
            a);
    };
    return MyComponent;
}());
GUI.render(GUI.createElement("group", { "$backgroundColor": "#000000f0" },
    GUI.createElement("hStack", { "$padding": 10 },
        GUI.createElement("group", { "$backgroundColor": "#0000ff6f", "$padding": 10, "$fontSize": 50, "$color": "tomato" },
            GUI.createElement("text", { "$backgroundColor": "#00ff00" }, "About Me"),
            GUI.createElement("spacer", null),
            GUI.createElement("text", { "$fontSize": 20, "$color": "#ff00ff" }, "Another String"),
            GUI.createElement("spacer", null),
            "Go and hire me!")),
    GUI.createElement("vStack", { "$backgroundColor": "green", "$padding": { horizontal: 100 }, "$hello": "aaaa", id: "parentDiv" },
        GUI.createElement("spacer", null),
        "This is a test!",
        GUI.createElement("spacer", null),
        GUI.createElement(MyComponent, { "$padding": { left: 12 } }),
        "This is a test!",
        GUI.createElement("spacer", null))));
setInterval(function () {
    print("After render...");
    print("Interval");
}, 3000);
// print(test, JSON.stringify(new MyComponent().render()))
