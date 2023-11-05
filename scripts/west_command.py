#
# gp-nrf52840-ble-blinky
# Copyright (c) 2023, Greg PFISTER. MIT Licennse
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#

Example of a west extension in the example-application repository.'''

from west.commands import WestCommand  # your extension must subclass this
from west import log                   # use this for user output

class ExampleWestCommand(WestCommand):

    def __init__(self):
        super().__init__(
            'example-west-command',               # gets stored as self.name
            'an example west extension command',  # self.help
            # self.description:
            '''\
A multi-line description of example-west-command.

You can split this up into multiple paragraphs and they'll get
reflowed for you. You can also pass
formatter_class=argparse.RawDescriptionHelpFormatter when calling
parser_adder.add_parser() below if you want to keep your line
endings.''')

    def do_add_parser(self, parser_adder):
        # This is a bit of boilerplate, which allows you full control over the
        # type of argparse handling you want. The "parser_adder" argument is
        # the return value of an argparse.ArgumentParser.add_subparsers() call.
        parser = parser_adder.add_parser(self.name,
                                         help=self.help,
                                         description=self.description)

        # Add some example options using the standard argparse module API.
        parser.add_argument('-o', '--optional', help='an optional argument')
        parser.add_argument('required', help='a required argument')

        return parser           # gets stored as self.parser

    def do_run(self, args, unknown_args):
        # This gets called when the user runs the command, e.g.:
        #
        #   $ west my-command-name -o FOO BAR
        #   --optional is FOO
        #   required is BAR
        log.inf('--optional is', args.optional)
        log.inf('required is', args.required)
