import { Slot } from "@radix-ui/react-slot";
import { cva, type VariantProps } from "class-variance-authority";
import type { ButtonHTMLAttributes } from "react";
import { cn } from "../../lib/utils";

const buttonVariants = cva(
  "inline-flex min-h-11 items-center justify-center gap-2 rounded-full border px-5 text-sm font-extrabold no-underline transition-colors focus-visible:outline-none focus-visible:ring-2 focus-visible:ring-[#59d9ef] disabled:pointer-events-none disabled:opacity-50",
  {
    variants: {
      variant: {
        default: "border-transparent bg-[#59d9ef] text-[#06131d] hover:bg-[#7be3f4]",
        outline:
          "border-[rgba(174,201,230,.16)] bg-transparent text-[#edf5ff] hover:border-[#59d9ef] hover:text-[#59d9ef]",
        link: "min-h-0 border-transparent p-0 text-[#59d9ef] hover:text-[#8ce8f7]",
      },
    },
    defaultVariants: { variant: "default" },
  },
);

type ButtonProps = ButtonHTMLAttributes<HTMLButtonElement> &
  VariantProps<typeof buttonVariants> & { asChild?: boolean };

export function Button({ className, variant, asChild = false, ...props }: ButtonProps) {
  const Component = asChild ? Slot : "button";
  return <Component className={cn(buttonVariants({ variant }), className)} {...props} />;
}
