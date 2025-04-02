#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100
#define MAX_POSTS 100
#define MAX_STR 100

typedef struct {
    int id;
    char username[MAX_STR];
    char password[MAX_STR];
    char email[MAX_STR];
} User;

typedef struct {
    int postId;
    int userId;
    char title[MAX_STR];
    char content[MAX_STR];
    char type[MAX_STR]; // "Picture" or "Video"
    int likes;
} Post;

// Function to delete a post
void deletePost(Post posts[], int *postCount, int userId) {
    int postId, i, found = 0;
    printf("Enter Post ID to delete: ");
    scanf("%d", &postId);

    for (i = 0; i < *postCount; i++) {
        if (posts[i].postId == postId && posts[i].userId == userId) {
            found = 1;
            for (int j = i; j < *postCount - 1; j++) {
                posts[j] = posts[j + 1];
            }
            (*postCount)--;
            printf("\n✅ Post deleted successfully!\n");
            return;
        }
    }
    if (!found) {
        printf("\n❌ Post not found or not owned by you.\n");
    }
}

// Function to like a post
void likePost(Post posts[], int postCount) {
    int postId, i, found = 0;
    printf("Enter Post ID to like: ");
    scanf("%d", &postId);

    for (i = 0; i < postCount; i++) {
        if (posts[i].postId == postId) {
            posts[i].likes++;
            printf("\n❤️ Post liked! Total likes: %d\n", posts[i].likes);
            return;
        }
    }
    printf("\n❌ Post not found.\n");
}

// Function to create a new post
void createPost(Post posts[], int *postCount, int userId) {
    if (*postCount >= MAX_POSTS) {
        printf("\n❌ Post limit reached.\n");
        return;
    }

    posts[*postCount].postId = *postCount + 1;
    posts[*postCount].userId = userId;
    posts[*postCount].likes = 0;

    printf("Enter Post Title: ");
    scanf(" %[^"]", posts[*postCount].title);
    printf("Enter Post Content: ");
    scanf(" %[^"]", posts[*postCount].content);

    printf("Choose Post Type (Picture/Video): ");
    scanf(" %s", posts[*postCount].type);

    (*postCount)++;
    printf("\n✅ Post created successfully!\n");
}

// Function to view posts
void viewPosts(Post posts[], int postCount) {
    if (postCount == 0) {
        printf("\n❌ No posts available.\n");
        return;
    }

    for (int i = 0; i < postCount; i++) {
        printf("\n🆔 ID: %d\n📌 Title: %s\n🎞️ Type: %s\n✏️ Content: %s\n❤️ Likes: %d\n",
               posts[i].postId, posts[i].title, posts[i].type, posts[i].content, posts[i].likes);
        printf("-----------------------------------\n");
    }
}

// Main function
int main() {
    int choice, userId = 1, postCount = 0;
    Post posts[MAX_POSTS];

    while (1) {
        printf("\n=== Menu ===\n");
        printf("1. Create Post\n2. View Posts\n3. Like Post\n4. Delete Post\n5. Exit\n> ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                createPost(posts, &postCount, userId);
                break;
            case 2:
                viewPosts(posts, postCount);
                break;
            case 3:
                likePost(posts, postCount);
                break;
            case 4:
                deletePost(posts, &postCount, userId);
                break;
            case 5:
                printf("Exiting...\n");
                return 0;
            default:
                printf("Invalid choice! Try again.\n");
        }
    }
}
