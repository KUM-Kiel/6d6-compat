library("tidyverse")

data <- read_csv("resampler-test.csv")

png("resampler-test.png", width = 8, height = 5, units = "in", res = 150)

ggplot(data) +
  aes(Time, Value) +
  geom_line()
