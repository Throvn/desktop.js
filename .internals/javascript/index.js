import * as GUI from "GUI";
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
        return GUI.createElement("text", { id: "container", width: 120, "$letterSpacing": 10 },
            "String",
            a);
    };
    return MyComponent;
}());
// os.setTimeout(changeColor, 30000)
var field = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
var grid = field.map(function (v, i) { return GUI.createElement("hStack", null, field.map(function (v, j) { return (GUI.createElement("vStack", { "$backgroundColor": "red" },
    "A ",
    i + j)); })); });
print(grid);
// print(test, JSON.stringify(new MyComponent().render()))
GUI.render((GUI.createElement("vStack", null,
    "Hello Bye What",
    grid)));
