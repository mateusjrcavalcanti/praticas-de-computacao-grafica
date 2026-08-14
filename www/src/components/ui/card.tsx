import type { HTMLAttributes } from "react";
import { cn } from "../../lib/utils";

export function Card({ className, ...props }: HTMLAttributes<HTMLElement>) {
  return (
    <article
      className={cn(
        "overflow-hidden rounded-[22px] border border-[rgba(174,201,230,.16)] bg-[linear-gradient(150deg,rgba(19,36,58,.9),rgba(9,20,35,.94))]",
        className,
      )}
      {...props}
    />
  );
}

export function CardHeader({ className, ...props }: HTMLAttributes<HTMLDivElement>) {
  return <div className={cn("p-[1.35rem] pb-0", className)} {...props} />;
}

export function CardContent({ className, ...props }: HTMLAttributes<HTMLDivElement>) {
  return <div className={cn("p-[1.35rem]", className)} {...props} />;
}
