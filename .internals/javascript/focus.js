/**
 * This program should display two boxes (left to right). If you click on the first one, it should have a red border, because it's focused. If you click on the second one, it should have a red border and the first one shouldn't have a border anymore.
 * The boxes also capture keyboard input and display it.
 */
import * as GUI from "GUI";
var RedBox = /** @class */ (function () {
    function RedBox(props) {
        this.props = props;
        this.color = "white";
    }
    RedBox.prototype.render = function () {
        var _this = this;
        return GUI.createElement("vStack", { "$backgroundColor": this.hasFocus ? "blue" : "gray", onFocus: function (e) {
                _this.hasFocus = true;
                console.log("Focus In", _this.props.children);
            }, onBlur: function (e) {
                _this.hasFocus = false;
                console.log("Focus Out", _this.props.children);
            }, onKeyPress: function (e) {
                _this.text += e.key;
                _this.color = "tomato";
            } }, this.text || this.props.children);
    };
    return RedBox;
}());
GUI.render(GUI.createElement("hStack", { "$gap": 5 },
    GUI.createElement(RedBox, null, "1"),
    GUI.createElement(RedBox, null, "2")));
