var __makeTemplateObject = (this && this.__makeTemplateObject) || function (cooked, raw) {
    if (Object.defineProperty) { Object.defineProperty(cooked, "raw", { value: raw }); } else { cooked.raw = raw; }
    return cooked;
};
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
    MyComponent.prototype.style = function () {
        return css(templateObject_1 || (templateObject_1 = __makeTemplateObject(["\n            * {\n                background-color: black;\n            }\n        "], ["\n            * {\n                background-color: black;\n            }\n        "])));
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
React.render(React.createElement(MyComponent, { props: "prop rendered!" }));
setInterval(function () {
    print("Interval");
}, 3000);
print(test, JSON.stringify(new MyComponent().render()));
var templateObject_1;
