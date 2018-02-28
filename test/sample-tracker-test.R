library(tidyverse)

data <- read_csv('sample-tracker-test.csv', col_types = 'dd')

cairo_pdf('sample-tracker-test.pdf~', width = 8, height = 5)

data %>%
  transmute(Time = Real * 1e-6, Error = Estimate - Real) %>%
  ggplot(aes(Time, Error)) +
  labs(
    title = 'Phase Error of the Sample Tracker',
    subtitle = 'The sample tracker was given a jittery clock with a small constant error.',
    x = 'Time [s]',
    y = 'Error [µs]'
  ) +
  geom_line()

file.rename('sample-tracker-test.pdf~', 'sample-tracker-test.pdf')
