import pyperclip

# Roughly based on the matplotlib viridis colormap
gradient_definition = [
    # (t, color)
    (0, (180, 0, 255)),
    (0.5, (0, 255, 180)),
    (1, (255, 255, 0))
]

def lerp(a, b, t):
    return (1 - t) * a + t * b

def sample_gradient(t, gradient):
    start_index = -1

    for stop in gradient:
        if stop[0] > t:
            break
        else:
            start_index += 1

    if start_index == -1:
        return gradient[0][1]
    elif start_index == len(gradient) - 1:
        return gradient[-1][1]

    start_t = gradient[start_index][0]
    end_t = gradient[start_index + 1][0]
    t = (t - start_t) / (end_t - start_t)

    return (
        int(lerp(gradient[start_index][1][0], gradient[start_index + 1][1][0], t)),
        int(lerp(gradient[start_index][1][1], gradient[start_index + 1][1][1], t)),
        int(lerp(gradient[start_index][1][2], gradient[start_index + 1][1][2], t))
    )

gradient_samples = 256
gradient_array = [sample_gradient(i / gradient_samples, gradient_definition) for i in range (gradient_samples)]

cpp_gradient_array = f"""constexpr int COLORMAP_SIZE = {gradient_samples};

// Generated using dev-tools/color-array-generator.py.
constexpr Color COLORMAP_ARRAY[COLORMAP_SIZE] = {{
"""

for sample in gradient_array:
    cpp_gradient_array += f"\tColor{{{sample[0]}, {sample[1]}, {sample[2]}, 255}},\n"

cpp_gradient_array = cpp_gradient_array[:-2] + "\n};"

desmos_gradient_array = "C=["

for sample in gradient_array:
    desmos_gradient_array += f'rgb{sample[0], sample[1], sample[2]}, '

desmos_gradient_array = desmos_gradient_array[:-2] + "]"

print("C++ gradient array:\n")
print(cpp_gradient_array)
print("\nDesmos array (for previewing at https://www.desmos.com/calculator/zcfsvtyile):")
print(desmos_gradient_array)

choice = input("\nCopy C++ (1) or Desmos (2) array?\n> ")
if choice == "1":
    pyperclip.copy(cpp_gradient_array)
elif choice == "2":
    pyperclip.copy(desmos_gradient_array + f"\nN={gradient_samples}")
