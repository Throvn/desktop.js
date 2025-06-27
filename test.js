import * as React from "./libgui.so";
console.log("JSX test started...");
var test = React.createElement("div", null, "Test");
// class MyComponent extends React.Component {
var MyComponent = /** @class */ (function () {
    function MyComponent(props) {
        this.props = props;
    }
    MyComponent.prototype.anotherFunc = function () {
        return true;
    };
    MyComponent.prototype.render = function () {
        return React.createElement("h1", { id: "container", width: 120 },
            React.createElement("h2", { id: "nested" },
                "Nested",
                React.createElement("h3", { id: "super nested" }, 1234)),
            React.createElement("h4", { id: "same level as nested" }));
    };
    return MyComponent;
}());
// React.render(<MyComponent props="prop rendered!" />)
React.render(React.createElement("text", null, "Hello"));
print(test, JSON.stringify(new MyComponent().render()));
