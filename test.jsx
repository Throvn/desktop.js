import * as React from "./libgui.so"
console.log("JSX test started...")
const test = <div>
    Test
</div>

// class MyComponent extends React.Component {
class MyComponent {
    constructor(props) {
        this.props = props
    }

    anotherFunc() {
        return true
    }

    style() {
        return css`
            * {
                background-color: black;
            }
        `
    }

    render() {
        return <h1 id="container" width={120}>
            <h2 id="nested">
                Nested
                <h3 id="super nested">
                    {1234}
                </h3>
            </h2>
            <h4 id="same level as nested">
            </h4>
        </h1>;
    }
}

React.render(<MyComponent props="prop rendered!" />)
setInterval(() => {
    print("Interval")
}, 3000)

print(test, JSON.stringify(new MyComponent().render()))
