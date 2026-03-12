#!/usr/bin/env python3
"""
Demo Script Generator for mycroft-gui-qt6

Generates a script that cycles through all 25 OVOS templates,
displaying each for 5 seconds to visually demonstrate the GUI.

Usage:
    python demo_script.py | nc localhost 18181
    
Or integrate with mock_gui_service.py for automated demo mode.
"""

import json
import sys
import time
from typing import List, Dict, Any

# All 25 OVOS templates with sample data
TEMPLATES = [
    # System group
    {
        'name': 'SYSTEM_idle',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'system',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Idle.qml', 'page': 'Idle.qml'}]
                },
                'context': {}
            }
        ]
    },
    {
        'name': 'SYSTEM_loading',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Loading.qml', 'page': 'Loading.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {'label': 'Loading weather data...'},
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    {
        'name': 'SYSTEM_status',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Status.qml', 'page': 'Status.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {'label': 'Operation completed!', 'success': True},
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    {
        'name': 'SYSTEM_error',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Error.qml', 'page': 'Error.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {'label': 'Connection failed', 'detail': 'Timeout after 30s'},
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    
    # Content group
    {
        'name': 'SYSTEM_text',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Text.qml', 'page': 'Text.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {
                    'text': 'The quick brown fox jumps over the lazy dog. This is a demonstration of the text template showing multiple paragraphs.',
                    'title': 'Sample Text'
                },
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    {
        'name': 'SYSTEM_image',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Image.qml', 'page': 'Image.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {
                    'image': 'https://upload.wikimedia.org/wikipedia/commons/4/41/Sunflower_from_Silesia2.jpg',
                    'title': 'Sunflower',
                    'caption': 'Beautiful sunflower field',
                    'fill': 'crop'
                },
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    {
        'name': 'SYSTEM_animated_image',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/AnimatedImage.qml', 'page': 'AnimatedImage.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {
                    'image': 'https://upload.wikimedia.org/wikipedia/commons/2/2e/Rotating_earth_%28large%29.gif',
                    'title': 'Earth Rotation',
                    'caption': 'Animated GIF demonstration'
                },
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    {
        'name': 'SYSTEM_list',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/List.qml', 'page': 'List.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {
                    'title': 'Shopping List',
                    'items': [
                        {'title': 'Milk', 'subtitle': '1 gallon'},
                        {'title': 'Eggs', 'subtitle': 'Dozen'},
                        {'title': 'Bread', 'subtitle': 'Whole wheat'},
                        {'title': 'Apples', 'subtitle': '5 lbs'}
                    ]
                },
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    {
        'name': 'SYSTEM_grid',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Grid.qml', 'page': 'Grid.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {
                    'title': 'Photo Gallery',
                    'items': [
                        {'image': 'https://picsum.photos/300/300?random=1', 'title': 'Photo 1'},
                        {'image': 'https://picsum.photos/300/300?random=2', 'title': 'Photo 2'},
                        {'image': 'https://picsum.photos/300/300?random=3', 'title': 'Photo 3'},
                        {'image': 'https://picsum.photos/300/300?random=4', 'title': 'Photo 4'}
                    ]
                },
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    {
        'name': 'SYSTEM_table',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Table.qml', 'page': 'Table.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {
                    'title': 'Weather Data',
                    'columns': ['City', 'Temperature', 'Condition'],
                    'rows': [
                        ['Berlin', '22°C', 'Sunny'],
                        ['London', '18°C', 'Cloudy'],
                        ['Paris', '25°C', 'Rain'],
                        ['Madrid', '30°C', 'Sunny']
                    ]
                },
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    {
        'name': 'SYSTEM_html',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Html.qml', 'page': 'Html.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {
                    'html': '<h1>HTML Template</h1><p>This <b>supports</b> <i>HTML</i> formatting.</p><ul><li>Item 1</li><li>Item 2</li></ul>',
                    'resource_url': 'https://example.com/'
                },
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    {
        'name': 'SYSTEM_url',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Url.qml', 'page': 'Url.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {'url': 'https://openvoiceos.com'},
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    
    # Media group
    {
        'name': 'SYSTEM_media_player',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/MediaPlayer.qml', 'page': 'MediaPlayer.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {
                    'ocp_title': 'Bohemian Rhapsody',
                    'ocp_artist': 'Queen',
                    'ocp_album': 'A Night at the Opera',
                    'ocp_image': 'https://upload.wikimedia.org/wikipedia/en/9/9f/Queen_Bohemian_Rhapsody.png',
                    'ocp_uri': 'spotify:track:0JW6RJWjBXjOz0jKKzL4Z',
                    'ocp_position': 125,
                    'ocp_duration': 354,
                    'ocp_playback_state': 'playing'
                },
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    
    # Utility group
    {
        'name': 'SYSTEM_clock',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Clock.qml', 'page': 'Clock.qml'}]
                },
                'context': {}
            }
        ]
    },
    {
        'name': 'SYSTEM_timer',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Timer.qml', 'page': 'Timer.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {
                    'end_time': int(time.time()) + 60,
                    'label': 'Pasta Timer',
                    'count_up': False
                },
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    {
        'name': 'SYSTEM_weather',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Weather.qml', 'page': 'Weather.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {
                    'current_temp': 22,
                    'min_temp': 18,
                    'max_temp': 25,
                    'condition': 'Partly Cloudy',
                    'icon': 'https://openweathermap.org/img/wn/02d.png',
                    'location': 'Berlin, Germany'
                },
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    {
        'name': 'SYSTEM_map',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Map.qml', 'page': 'Map.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {
                    'latitude': 52.5200,
                    'longitude': 13.4050,
                    'zoom': 12,
                    'label': 'Berlin, Germany'
                },
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    
    # Dialogue group
    {
        'name': 'SYSTEM_confirm',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Confirm.qml', 'page': 'Confirm.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {'question': 'Do you want to delete this file?'},
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    {
        'name': 'SYSTEM_select',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'test-skill',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Select.qml', 'page': 'Select.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {
                    'prompt': 'Choose an option',
                    'items': [
                        {'label': 'Option 1', 'value': 'opt1'},
                        {'label': 'Option 2', 'value': 'opt2'},
                        {'label': 'Option 3', 'value': 'opt3'}
                    ]
                },
                'context': {'skill_id': 'test-skill'}
            }
        ]
    },
    
    # Avatar group
    {
        'name': 'SYSTEM_face',
        'messages': [
            {
                'type': 'mycroft.gui.list.insert',
                'data': {
                    'namespace': 'system',
                    'position': 0,
                    'data': [{'url': 'qrc:///qt5/Face.qml', 'page': 'Face.qml'}]
                },
                'context': {}
            },
            {
                'type': 'mycroft.session.set',
                'data': {'sleeping': False},
                'context': {'skill_id': 'system'}
            }
        ]
    }
]


def generate_demo_script() -> List[Dict[str, Any]]:
    """Generate complete demo script with all templates."""
    script = []
    
    # Add connected message
    script.append({
        'type': 'mycroft.gui.connected',
        'data': {'version': '1.0'},
        'context': {}
    })
    
    # Add all templates
    for template in TEMPLATES:
        # Add namespace activation
        script.append({
            'type': 'mycroft.session.list.insert',
            'data': {'namespace': 'mycroft.system.active_skills', 'position': 0, 'data': [{'skill_id': 'test-skill'}]},
            'context': {}
        })
        
        # Add template messages
        for msg in template['messages']:
            script.append(msg)
        
        # Add delay (simulated by client-side timing)
        # In real demo, we'll add a 5-second pause between templates
        
    return script


def print_demo_script():
    """Print demo script to stdout for piping to netcat."""
    script = generate_demo_script()
    
    print(f"// Demo Script: {len(TEMPLATES)} templates", file=sys.stderr)
    print(f"// Generated: {datetime.now()}", file=sys.stderr)
    
    for i, msg in enumerate(script):
        print(json.dumps(msg))
        sys.stdout.flush()
        # Small delay between messages for network
        time.sleep(0.1)


def get_template_names() -> List[str]:
    """Get list of all template names."""
    return [t['name'] for t in TEMPLATES]


if __name__ == '__main__':
    if '--list' in sys.argv:
        print("Available templates:")
        for name in get_template_names():
            print(f"  - {name}")
    else:
        print_demo_script()
