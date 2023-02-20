import re


def run_main(date_period_input):
    date_period = 1
    if isinstance(date_period_input, str):
        if re.search('[0-9]+', date_period):
            date_period = int(date_period_input)
    elif isinstance(date_period_input, int):
        date_period = date_period_input

    from gui import gui
    from ReleaseNoter import ReleaseNoter
    noter = ReleaseNoter(date_period)

    g = gui(noter)
    g.set_commit_data()
    g.mk_btn()
    g.run()
